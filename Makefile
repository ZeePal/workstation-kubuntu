#!/usr/bin/env make
COMMAND := sudo ./run_ansible_roles_in_parallel

.DEFAULT_GOAL := check
.PHONY: check
check:
	$(COMMAND) --check

.PHONY: verbose-check
verbose-check:
	$(COMMAND) --check -vvv


.PHONY: install
install:
	$(COMMAND)

.PHONY: verbose-install
verbose-install:
	$(COMMAND) -vvv
