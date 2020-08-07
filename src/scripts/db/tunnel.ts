import {execSync} from 'child_process'

import Database from '../../config/Db'
import * as Environment from '../../config/Env'

const {EnvKeys, getEnv} = Environment

const main = async () => {
  const port = Number(getEnv(EnvKeys.Port, '1701'))
  const key = getEnv(EnvKeys.SshKeyPath)
  const bastion = getEnv(EnvKeys.BastionHost)

  if (!bastion) {
    throw new Error(
      `Unable to locate a bastion server for ${Database.database}`
    )
  }

  console.log(
    new Date(),
    `Opening SSH tunnel to ${Database.database} database...`
  )

  execSync(
    `ssh -i ${key} -N -L ${port}:${Database.host}:${Database.port} ec2-user@${bastion}`,
    {stdio: 'inherit'}
  )
}

if (require.main === module) {
  main().catch((err) => {
    console.error(err)

    process.exit(1)
  })
}
