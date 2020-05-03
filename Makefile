#!/usr/bin/env make
COMMAND := sudo ansible-playbook --connection=local -i localhost, workstation.yml \
 	-e "primary_username=${USER}" -e ansible_python_interpreter=/usr/bin/python3

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
