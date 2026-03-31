# agentwrap
`agentwrap` is a small wrapper stack for running tools with layered containment.
The public interfaces are:

- `files/usr/local/bin/agentwrap`: full wrapper with network isolation + privilege boundary + `bubblewrap`
- `files/usr/local/bin/agentwrap-bwrap`: filesystem/env sandbox only

## Security Model
`agentwrap` composes several narrow controls instead of relying on one mechanism:

```text
user command
    |
    v
agentwrap
    |
    +--> passthrough env staging
    |      - selected vars are re-prefixed as AGENTWRAP_PASS_*
    |      - only the allowlisted bridge vars cross sudo
    |
    +--> sudo policy
    |      - only /usr/local/bin/agentwrap-root is allowed
    |      - env_reset + explicit env_keep allowlist
    |
    v
agentwrap-root
    |
    +--> ip netns exec agentwrap-egress
    |      - traffic forced through dedicated egress namespace
    |      - nftables filters/NAT enforce outbound policy
    |
    +--> setpriv --reuid/--regid --no-new-privs
    |      - root is used only to enter the namespace
    |      - payload runs again as the calling user
    |
    v
agentwrap-unroot
    |
    +--> passthrough env restore
    |      - unwraps AGENTWRAP_PASS_* back into selected vars
    |      - restores only the limited pre-bwrap handoff set
    |
    v
agentwrap-bwrap
    |
    +--> bubblewrap sandbox
           - unshared namespaces (`--unshare-all`, with shared net)
           - read-only system paths
           - writable workspace + selected state dirs only
           - environment stripped to an allowlist
```

## Components
- `tasks/main.yml`: installs binaries, config, sudoers, nftables, namespace DNS config, and the `agentwrap-egress.service`
- `files/usr/local/bin/agentwrap`: collects the small set of values allowed across `sudo` and invokes `agentwrap-root`
- `files/usr/local/bin/agentwrap-root`: enters the shared network namespace, then immediately drops back to the caller with `setpriv --no-new-privs`
- `files/usr/local/bin/agentwrap-unroot`: restores the limited cross-boundary environment and hands off to `agentwrap-bwrap`
- `files/usr/local/bin/agentwrap-bwrap`: builds the `bubblewrap` sandbox, injects optional cloud creds, and strips the final environment
- `files/opt/agentwrap/agentwrap.conf`: central policy/config for paths, env allowlists, extra binds, and namespace naming
- `files/etc/nftables.conf` and `files/etc/nftables.d/agentwrap.nft`: host firewall defaults plus the dedicated agentwrap egress ruleset
- `files/etc/systemd/system/agentwrap-egress.service`: keeps the shared namespace and veth pair present across boots/reloads
- `files/opt/agentwrap/bin/agentwrap-egress-init`: creates the namespace, veth pair, routes, and IPv6 posture
- `files/etc/systemd/resolved.conf.d/agentwrap.conf` and `files/etc/netns/agentwrap-egress/resolv.conf`: ensure DNS from the namespace resolves via the host-side listener
- `files/etc/sudoers.d/agentwrap`: constrains the `sudo` entrypoint and resets environment by default
- `files/opt/agentwrap/bin/agentwrap-check`: root-side verification of namespace and nftables state
- `files/opt/agentwrap/bin/agentwrap-test`: behavioral checks for reachability, path visibility, and write permissions

## Protection Layers
- `sudoers` restricts elevation to one wrapper entrypoint and resets the environment first.
- `setpriv --no-new-privs` ensures the payload cannot regain privilege after namespace entry.
- `ip netns exec agentwrap-egress` forces wrapped traffic through a dedicated egress path.
- `nftables` blocks private/special-use ranges by default and limits outbound traffic to approved uplinks.
- `systemd-resolved` exposes DNS on the host-side veth so namespace DNS does not bypass host policy.
- `bubblewrap` makes the workspace the primary writable surface; system paths are read-only and most of `$HOME` is synthetic/empty.
- Final environment stripping limits what secrets and host context survive into the sandbox.

## Operational Notes
- The network namespace name is intentionally hardcoded in `agentwrap-root`; see the note in `files/opt/agentwrap/agentwrap.conf`.
- `agentwrap-bwrap` is intentionally weaker than `agentwrap` because it shares the caller's network namespace.
- Private IPv4 allowlists in `files/etc/nftables.d/agentwrap.nft` are empty by default; add narrow exceptions there if needed.

## Validation
- `files/opt/agentwrap/bin/agentwrap-check`
- `files/opt/agentwrap/bin/agentwrap-test`
