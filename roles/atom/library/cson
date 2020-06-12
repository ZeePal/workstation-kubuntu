#!/usr/bin/python3
import cson

from ansible.module_utils.basic import AnsibleModule


def update(data, key_paths, value):
    key = key_paths[0]
    remaining = key_paths[1:]
    if remaining:
        if key not in data:
            data[key] = {}
        return update(data[key], remaining, value)
    else:
        if key not in data:
            data[key] = value
            return True
        elif isinstance(data[key], list):
            if isinstance(value, list):
                changed = False
                for item in value:
                    if item not in data[key]:
                        data[key].append(item)
                        changed = True
                return changed
            else:
                raise ValueError("Target is a list but source value isn't")
        elif isinstance(data[key], dict):
            if isinstance(value, dict):
                changed = False
                for item_key, item_value in value.items():
                    if item_value != data[key].get(item_key, None):
                        data[key][item_key] = item_value
                        changed = True
                return changed
            else:
                raise ValueError("Target is a dict but source value isn't")
        else:
            if data[key] != value:
                data[key] = value
                return True
            return False


def load_cson(file_path):
    with open(file_path, "r") as f:
        return cson.load(f)


def save_cson(data, file_path):
    with open(file_path, "w") as f:
        cson.dump(data, f, indent=4, sort_keys=True)


def main():
    module_args = dict(
        file=dict(type="str", required=True),
        create=dict(type="bool", default=False),
        changes=dict(type="list", required=True),
    )

    result = dict(changed=False)
    module = AnsibleModule(argument_spec=module_args, supports_check_mode=True)

    try:
        data = load_cson(module.params["file"])
    except FileNotFoundError as e:
        if not module.params["create"]:
            raise e
        data = {}
    for change in module.params["changes"]:
        if update(data, change["path"].split("."), change["value"]):
            result["changed"] = True

    if not module.check_mode and result["changed"]:
        save_cson(data, module.params["file"])

    module.exit_json(**result)


if __name__ == "__main__":
    main()
