from ansible_collections.community.general.plugins.callback.yaml import CallbackModule as CommunityYaml

DOCUMENTATION = """
    name: yaml_quiet
    type: stdout
    short_description: quieter yaml
    description:
        - A less verbose yaml.
    extends_documentation_fragment:
        - default_callback
    requirements:
        - set as stdout in configuration
"""


class CallbackModule(CommunityYaml):
    CALLBACK_VERSION = 2.0
    CALLBACK_TYPE = "stdout"
    CALLBACK_NAME = "yaml_quiet"

    def __init__(self):
        super().__init__()

    def v2_playbook_on_start(self, playbook):
        if self._display.verbosity != 0:
            return super().v2_playbook_on_start(playbook)

    def v2_playbook_on_play_start(self, play):
        if self._display.verbosity != 0:
            return super().v2_playbook_on_play_start(play)
        else:
            self._play = play

    def _print_task_banner(self, task):
        if self._display.verbosity != 0 or (task._role is not None and task.action != "include_role"):
            return super()._print_task_banner(task)
        else:
            self._last_task_banner = task._uuid

    def v2_playbook_on_stats(self, stats):
        if self._display.verbosity != 0:
            return super().v2_playbook_on_stats(stats)
