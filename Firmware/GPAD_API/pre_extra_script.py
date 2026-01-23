Import("env")

try:
    import git
except ImportError:
    env.Execute("$PYTHONEXE -m pip install GitPython")

repo = git.Repo('../../')
tags = sorted(repo.tags, key=lambda t: t.commit.committed_datetime)
latest_tag = "unknown"
if len(tags) > 0:
    latest_tag = [-1]

cpp_defines = [
    ("COMPANY_NAME", "PubInv "),   # For the Broker ID for MQTT 
    ("PROG_NAME", "GPAD_API "),    # This program
    ("FIRMWARE_VERSION", f"{latest_tag} "),
    ("MODEL_NAME", "KRAKE_"), 
    ("LICENSE", "GNU Affero General Public License, version 3 "),
    ("ORIGIN", "US"),
]

stringify_lambda = lambda macro: (macro[0], env.StringifyMacro(macro[1]))

env.Append(CPPDEFINES=map(stringify_lambda, cpp_defines))
