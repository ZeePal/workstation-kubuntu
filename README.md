# ZeePal's Kubuntu Workstation Installer
This repo is intended to provide an easy way of building my preferred workstation.

Installation is intended to run on a brand new installation of Ubuntu 22.04.


## Lazy Installation
**DISCLAIMER:** I do not recommend using this method, its INSECURE!
```bash
wget -O - https://raw.githubusercontent.com/ZeePal/workstation-kubuntu/master/bootstrap_install | bash
```

## Safer Installation
```bash
wget -O /tmp/bootstrap https://raw.githubusercontent.com/ZeePal/workstation-kubuntu/master/bootstrap_install
less /tmp/bootstrap
# Use eyeballs to review :D
chmod +x /tmp/bootstrap
/tmp/bootstrap
```


# Checklist after above
- GPG Key
- KDE Wallet
- Chrome
- SSH Key

