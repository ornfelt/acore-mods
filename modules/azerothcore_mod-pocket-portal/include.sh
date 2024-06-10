#!/usr/bin/env bash
	
	MOD_POCKET_PORTAL_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"
	
	source $MOD_POCKET_PORTAL_ROOT"/conf/conf.sh.dist"
	
	if [ -f $MOD_POCKET_PORTAL_ROOT"/conf/conf.sh" ]; then
	    source $MOD_POCKET_PORTAL_ROOT"/conf/conf.sh"
	fi
