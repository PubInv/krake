Import("env")

from pathlib import Path
import re

version_path = Path(env["PROJECT_DIR"]) / "FIRMWARE_VERSION"
firmware_version = version_path.read_text(encoding="utf-8").strip()
if not re.fullmatch(r"(?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*)", firmware_version):
    raise ValueError(f"Invalid plain semantic version in {version_path}: {firmware_version!r}")

cpp_defines = [
    ("COMPANY_NAME", "PubInv "),   # For the Broker ID for MQTT 
    ("PROG_NAME", "GPAD_API "),    # This program
    ("FIRMWARE_VERSION", firmware_version),
    ("LittleFS_VERSION", "0.1.8 "), # pr 569
    ("MODEL_NAME", "KRAKE_"), 
    ("LICENSE", "GNU Affero General Public License, version 3 "),
    ("ORIGIN", "US"),
]

stringify_lambda = lambda macro: (macro[0], env.StringifyMacro(macro[1]))

env.Append(CPPDEFINES=map(stringify_lambda, cpp_defines))
