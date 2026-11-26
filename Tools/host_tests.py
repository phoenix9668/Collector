"""Build and run the host unit tests in Tests/ with whatever is installed.

usage: python Tools/host_tests.py [--quecpython <Collector-QuecPython checkout>]

C compiler: $CC, else gcc / clang / cc on PATH, else the zig compiler from the
`ziglang` pip package (python -m pip install ziglang).
CMake / Ninja: on PATH, else the copies the STM32Cube VS Code extension keeps
in %LOCALAPPDATA%/stm32cube/bundles.
With --quecpython (default: ../Collector-QuecPython if it exists) the frames
are also decoded by the 4G firmware's parser (Tools/crosscheck_frames.py).
"""
import argparse
import glob
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def find_tool(name):
    path = shutil.which(name)
    if path:
        return path
    bundles = os.path.join(os.environ.get("LOCALAPPDATA", ""), "stm32cube", "bundles", name, "*", "bin")
    for candidate in sorted(glob.glob(os.path.join(bundles, name + "*")), reverse=True):
        if os.path.basename(candidate).split(".")[0] == name:
            return candidate
    sys.exit("%s not found (install it or the STM32Cube VS Code extension)" % name)


def find_cc():
    if os.environ.get("CC"):
        return os.environ["CC"]
    for name in ("gcc", "clang", "cc"):
        if shutil.which(name):
            return name
    try:
        import ziglang  # noqa: F401  (pip install ziglang)
    except ImportError:
        sys.exit("no host C compiler: set CC, install gcc/clang, or: python -m pip install ziglang")
    zig = (Path(ziglang.__file__).parent / ("zig.exe" if os.name == "nt" else "zig")).as_posix()
    # CMake splits "program args" in CC only for a forward-slash path.
    return '"%s" cc' % zig if " " in zig else "%s cc" % zig


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--quecpython", help="Collector-QuecPython checkout for the frame cross-check")
    args = ap.parse_args()

    quec = args.quecpython
    sibling = ROOT.parent / "Collector-QuecPython"
    if quec is None and (sibling / "src" / "collector_protocol.py").exists():
        quec = str(sibling)

    cmake, ninja, cc = find_tool("cmake"), find_tool("ninja"), find_cc()
    build = ROOT / "Tests" / "build" / "host"
    env = dict(os.environ, CC=cc)
    configure = [cmake, "-S", str(ROOT / "Tests"), "-B", str(build), "-G", "Ninja",
                 "-DCMAKE_MAKE_PROGRAM=" + ninja, "-DCMAKE_BUILD_TYPE=Debug",
                 "-DQUECPYTHON_DIR=" + (quec or "")]
    print("CC =", cc)
    subprocess.run(configure, check=True, env=env)
    subprocess.run([cmake, "--build", str(build)], check=True, env=env)
    ctest = str(Path(cmake).with_name("ctest" + Path(cmake).suffix))
    return subprocess.run([ctest, "--test-dir", str(build), "--output-on-failure"], env=env).returncode


if __name__ == "__main__":
    sys.exit(main())
