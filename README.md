# ZeePal's Ubuntu Workstation Installer
This repo is intended to provide an easy way of building my preferred workstation.

Installation is intended to run on a brand new installation of Ubuntu 20.04.


## Lazy Installation
**DISCLAIMER:** I do not recommend using this method, its INSECURE!
```bash
curl https://raw.githubusercontent.com/ZeePal/workstation-ubuntu/master/bootstrap_install | bash
```

## Safer Installation
```bash
curl https://raw.githubusercontent.com/ZeePal/workstation-ubuntu/master/bootstrap_install > /tmp/bootstrap
less /tmp/bootstrap
# Use eyeballs to review :D
chmod +x /tmp/bootstrap
/tmp/bootstrap
```
