"""Decode the firmware's B0/B2 frames with the 4G firmware's own parser.

usage: crosscheck_frames.py --tests <collector_tests> --quecpython <checkout> [--work <dir>]

Runs the host test binary with --dump to get frames built by App/protocol, then
feeds them through Collector-QuecPython's src/collector_protocol.py (the parser
the ESP32 adapter port and the PC station also follow) and checks every field.
Works with the feature/rs485-local branch and main.
"""
import argparse
import subprocess
import sys
import tempfile
import zlib
from pathlib import Path


def load_frames(path):
    frames = []
    for line in Path(path).read_text().splitlines():
        parts = line.split()
        if not parts:
            continue
        kind, data = parts[0], bytes.fromhex(parts[1])
        meta = dict(p.split("=", 1) for p in parts[2:])
        frames.append((kind, data, meta))
    return frames


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tests", required=True, help="path to the collector_tests binary")
    ap.add_argument("--quecpython", required=True, help="Collector-QuecPython checkout")
    ap.add_argument("--work", help="directory for frames.txt (default: a temp dir)")
    args = ap.parse_args()

    work = Path(args.work) if args.work else Path(tempfile.mkdtemp())
    dump = work / "frames.txt"
    tests = str(Path(args.tests).resolve())
    subprocess.run([tests, "--dump", str(dump)], check=True, stdout=subprocess.DEVNULL)

    sys.path.insert(0, str(Path(args.quecpython) / "src"))
    import collector_protocol as cp  # noqa: E402

    frames = load_frames(dump)
    errors = []

    def check(cond, what):
        if not cond:
            errors.append(what)

    # 1. The stream parser splits our frames exactly, with status text between.
    status = b"##crosscheck##"
    blob = b"".join(f[1] for f in frames if f[0] == "B0") + status + b"".join(f[1] for f in frames if f[0] == "B2")
    stream = cp.FrameStream()
    stream.feed(blob)
    got = {"signs": [], "voltage": [], "info": []}
    stream.drain(
        lambda fr, ms: got["signs"].append(bytes(fr)),
        lambda fr, ms: got["voltage"].append(bytes(fr)),
        lambda fr, ms: got["info"].append(bytes(fr)),
    )
    check(got["signs"] == [f[1] for f in frames if f[0] == "B0"], "B0 frames not split exactly")
    check(got["voltage"] == [f[1] for f in frames if f[0] == "B2"], "B2 frames not split exactly")
    check(got["info"] == [status], "status frame not found")
    check(stream.sync_errors == 0, "sync errors: %d" % stream.sync_errors)

    # 2. Field by field.
    for kind, data, meta in frames:
        if kind == "B0":
            check(len(data) == cp.SIGNS_FRAME_SIZE, "B0 length %d" % len(data))
            d = cp.decode_signs_frame(data, 0)
            check(d["collar_information:CollectorID"]["value"] == cp.legacy_hex(data[0:4]), "B0 CollectorID")
            check(d["collar_information:RFID"]["value"] == cp.legacy_hex(bytes.fromhex(meta["rfid"])), "B0 RFID")
            check(d["collar_information:Stage"]["value"] == int(meta["stage"]), "B0 stage")
            check(d["collar_information:ResetCnt"]["value"] == int(meta["reset"]), "B0 reset count")
            check(d["collar_information:SignalStrength"]["value"] == cp.calc_rssi_dbm(int(meta["rssi_raw"])),
                  "B0 RSSI")
            # CRC-32 over bytes 0..199, as the simulated STM32 in pc/ computes it.
            check(zlib.crc32(data[:200]) == int.from_bytes(data[200:204], "big"), "B0 CRC-32")
            check(data[204:206] == b"\r\n", "B0 tail")
        elif kind == "B2":
            d = cp.decode_voltage_frame(data)
            bat, pwr = int(meta["battery_code"]), int(meta["power_code"])
            check(d["BatteryVoltage"] == float("%.3f" % (bat / 32768.0 * 4.096 * 11)), "B2 battery %r" % d)
            check(d["PowerVoltage"] == float("%.3f" % (pwr / 32768.0 * 4.096 * 21)), "B2 power %r" % d)
            if bat == 7705:  # unit 062 on battery, from the 4.1.6 hand-off notes
                check(d["BatteryVoltage"] == 10.594 and d["PowerVoltage"] == 0.034, "B2 unit-062 values %r" % d)

    n_b0 = sum(1 for f in frames if f[0] == "B0")
    n_b2 = sum(1 for f in frames if f[0] == "B2")
    if errors:
        for e in errors:
            print("FAIL:", e)
        return 1
    print("crosscheck OK: %d B0 and %d B2 frames decode as expected with %s" % (n_b0, n_b2, cp.__file__))
    return 0


if __name__ == "__main__":
    sys.exit(main())
