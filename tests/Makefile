VM_NAME ?= test-workstation-kubuntu
VM_FOLDER ?= $(PWD)/vm
VM_DISK ?= $(VM_FOLDER)/$(VM_NAME).root
VM_ISO ?= $(HOME)/Downloads/ubuntu-24.04-live-server-amd64.iso


.PHONY: start-test # ~19min to run
start-test: $(VM_FOLDER)/$(VM_NAME)/$(VM_NAME).vbox
	VBoxManage controlvm "$(VM_NAME)" poweroff || true
	rm -rf "$(VM_FOLDER)/output"
	mkdir -p "$(VM_FOLDER)/output"
	VBoxManage snapshot "$(VM_NAME)" restore post-build

	VBoxManage startvm "$(VM_NAME)"


.PHONY: build-vm # ~18min to run
build-vm: $(VM_FOLDER)/$(VM_NAME)/$(VM_NAME).vbox
$(VM_FOLDER)/$(VM_NAME)/$(VM_NAME).vbox:
	mkdir -p "$(VM_FOLDER)/output"
	VBoxManage createvm --register \
		--name "$(VM_NAME)" \
		--basefolder "$(VM_FOLDER)"

	# Configure VM hardware
	VBoxManage modifyvm "$(VM_NAME)" \
		--description https://github.com/ZeePal/workstation-kubuntu \
		--ostype Ubuntu_64 \
		--memory 4096 --nestedpaging on \
		--cpus 4 --cpu-profile host --pae on --longmode on \
		--acpi on --ioapic on --x2apic on --firmware efi --rtcuseutc on \
		--boot1 dvd --boot2 disk --boot3 none --boot4 none \
		--nic1 nat --nictype1 82540EM \
		--graphicscontroller vmsvga --vram 128 --accelerate3d on --monitorcount 1 \
		--audio pulse --audiocodec ad1980 --audioin off --audioout on \
		--mouse usbtablet --usbohci on \
		--clipboard-mode disabled --draganddrop disabled

	# Add new root disk to VM
	VBoxManage storagectl "$(VM_NAME)" \
		--name "SATA Controller" \
		--add sata --portcount 3 \
		--hostiocache on --bootable on
	VBoxManage createmedium disk --size 30720 \
		--filename "$(VM_DISK)"
	VBoxManage storageattach "$(VM_NAME)" --storagectl "SATA Controller" \
		--type hdd --port 0 --device 0 --nonrotational on \
		--medium "$(VM_DISK)"

	# Attach installation ISO
	VBoxManage storageattach "$(VM_NAME)" --storagectl "SATA Controller" \
		--type dvddrive --port 1 --device 0 \
		--medium "$(VM_ISO)"

	# Generate user_data.iso & Attach
	cloud-localds user_data.iso user_data.yaml
	VBoxManage storageattach "$(VM_NAME)" --storagectl "SATA Controller" \
		--type dvddrive --port 2 --device 0 \
		--medium "$(PWD)/user_data.iso"

	# Configure Shared Folders between VM & Host
	VBoxManage sharedfolder add "$(VM_NAME)" \
		--name workstation-kubuntu \
		--hostpath "$(PWD)/.." \
		--auto-mount-point /mnt/workstation-kubuntu \
		--automount --readonly
	VBoxManage sharedfolder add "$(VM_NAME)" \
		--name output \
		--hostpath "$(VM_FOLDER)/output" \
		--auto-mount-point /mnt/output \
		--automount

	# Start autoinstall & First boot for user_data
	VBoxManage startvm "$(VM_NAME)"
	while true; do \
		if VBoxManage list runningvms | grep -q "$(VM_NAME)"; then \
			echo 'Waiting for autoinstall & user_data to finish... (Manually say "yes" to "Continue with autoinstall (yes|no)?")'; \
			sleep 10; \
		else \
			break; \
		fi \
	done

	# Detach Install & user_data ISO's
	VBoxManage storageattach "$(VM_NAME)" --storagectl "SATA Controller" \
		--type dvddrive --port 1 --device 0 \
		--medium none
	VBoxManage storageattach "$(VM_NAME)" --storagectl "SATA Controller" \
		--type dvddrive --port 2 --device 0 \
		--medium none

	# Snapshot ready for testing VM
	VBoxManage snapshot "$(VM_NAME)" take post-build


.PHONY: clean
clean:
	VBoxManage unregistervm --delete "$(VM_NAME)" || true
	VBoxManage closemedium --delete disk "$(VM_DISK)" || true
	rm -rf "$(PWD)/vm"
