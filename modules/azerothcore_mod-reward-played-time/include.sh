#!/usr/bin/env bash
	
MOD_REWARD_SYSTEM_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"
	
source $MOD_REWARD_SYSTEM_ROOT"/conf/conf.sh.dist"
	
if [ -f $MOD_REWARD_SYSTEM_ROOT"/conf/conf.sh" ]; then
    source $MOD_REWARD_SYSTEM_ROOT"/conf/conf.sh"
fi
