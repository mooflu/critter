#!/usr/bin/env python3
import subprocess
import json

# must set deployment status to inactive before delete
deploymentsCmd = '''gh api -H "Accept: application/vnd.github+json" -H "X-GitHub-Api-Version: 2022-11-28" /repos/mooflu/critter/deployments'''
updateStatusCmd = '''gh api --method POST -H "Accept: application/vnd.github+json" -H "X-GitHub-Api-Version: 2022-11-28" /repos/mooflu/critter/deployments/{id}/statuses -f state='inactive' '''
deleteCmd = '''gh api --method DELETE -H "Accept: application/vnd.github+json" -H "X-GitHub-Api-Version: 2022-11-28" /repos/mooflu/critter/deployments/{id}'''

deployments = json.loads(subprocess.check_output(deploymentsCmd, shell=True).decode("utf-8"))

# delete al but first two - seems to be ordered desc by created
for dep in deployments[2:]:
    print(dep['id'], dep['created_at'])

    #set to inactive
    print(subprocess.check_output(updateStatusCmd.format(id=dep['id']), shell=True).decode("utf-8"))

    # delete inactive
    print(subprocess.check_output(deleteCmd.format(id=dep['id']), shell=True).decode("utf-8"))
