#!/usr/bin/env bash

MOD_IP2NATION_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source "$MOD_IP2NATION_ROOT/conf/conf.sh.dist"

if [ -f "$MOD_IP2NATION_ROOT/conf/conf.sh" ]; then
    source "$MOD_IP2NATION_ROOT/conf/conf.sh"
fi
