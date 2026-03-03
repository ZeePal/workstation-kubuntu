from ansible import constants as C
from ansible.executor.task_result import CallbackTaskResult
from ansible.playbook.task_include import TaskInclude
from ansible.plugins.callback.default import CallbackModule as CallbackDefaultModule

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


class CallbackModule(CallbackDefaultModule):
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

    def v2_runner_on_ok(self, result: CallbackTaskResult) -> None:
        if isinstance(result.task, TaskInclude):
            return super().v2_runner_on_ok(result)
        elif result.result.get("changed", False):
            if self._last_task_banner != result.task._uuid:
                self._print_task_banner(result.task)

            msg = ""
            if "cmd" in result.result:
                if "stdout" in result.result:
                    msg += result.result["stdout"]
                if "stderr" in result.result:
                    msg += result.result["stderr"]
            if msg:
                self._display.display(msg)
        return super().v2_runner_on_ok(result)

    def v2_runner_on_failed(self, result: CallbackTaskResult, ignore_errors: bool = False) -> None:
        if self._last_task_banner != result.task._uuid:
            self._print_task_banner(result.task)

        msg = ""
        if "cmd" in result.result:
            if "stdout" in result.result:
                msg += result.result["stdout"]
            if "stderr" in result.result:
                msg += result.result["stderr"]
        if msg:
            self._display.display(msg)
        return super().v2_runner_on_failed(result, ignore_errors=ignore_errors)
