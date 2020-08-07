import {execSync} from 'child_process'

import Database from '../../config/Db'

export async function main(): Promise<void> {
  console.log(new Date(), `Creating database ${Database.database}...`)

  execSync(
    `PGPASSWORD=${Database.password} createdb -h ${Database.host} -p ${Database.port} -U ${Database.username} -O ${Database.username} ${Database.database}`,
    {stdio: ['inherit', 'pipe', 'inherit']}
  )
}

if (require.main === module) {
  main()
    .catch(console.error) // tslint:disable-line:no-console
    .finally(process.exit)
}
