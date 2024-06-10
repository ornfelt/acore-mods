#!/usr/bin/env bash
	
	MOD_CHARACTER_TOOLS_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"
	
	source $MOD_CHARACTER_TOOLS_ROOT"/conf/conf.sh.dist"
	
	if [ -f $MOD_CHARACTER_TOOLS_ROOT"/conf/conf.sh" ]; then
	    source $MOD_CHARACTER_TOOLS_ROOT"/conf/conf.sh"
	fi
