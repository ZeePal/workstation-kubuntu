#!/usr/bin/python3
import re

from ansible.module_utils.basic import AnsibleModule


PACKAGE_PATTERN = re.compile(r"^(\S+)@", re.MULTILINE)


def get_installed_packages(run, apm_path):
    _, stdout, _ = run([apm_path, "list", "-b"])
    return set(PACKAGE_PATTERN.findall(stdout))


def install_packages(run, apm_path, packages):
    run([apm_path, "install"] + packages, check_rc=True)


def main():
    module_args = dict(name=dict(type="list", required=True))

    result = dict(changed=False)
    module = AnsibleModule(argument_spec=module_args, supports_check_mode=True)

    apm_path = module.get_bin_path("apm", True)
    to_install = set(module.params["name"])
    installed = get_installed_packages(module.run_command, apm_path)

    missing = list(to_install - installed)
    if missing:
        result["changed"] = True
        if not module.check_mode:
            install_packages(module.run_command, apm_path, missing)

    result["already_installed"] = list(to_install & installed)
    result["missing_packages"] = missing
    module.exit_json(**result)


if __name__ == "__main__":
    main()
