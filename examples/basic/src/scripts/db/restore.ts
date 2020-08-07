import {execSync} from 'child_process'

import Database from '../../config/Db'

const main = async () => {
  const sqlFile = process.argv[2]

  if (!sqlFile) throw new Error('No filename was provided to restore from.')

  console.log(new Date(), `Beginning ${Database.database} database restore...`)

  execSync(
    `PGPASSWORD=${Database.password} dropdb --if-exists -h ${Database.host} -p ${Database.port} -U ${Database.username} ${Database.database}`,
    {stdio: 'inherit'}
  )

  execSync(
    `PGPASSWORD=${Database.password} createdb -h ${Database.host} -p ${Database.port} -U ${Database.username} ${Database.database} -O ${Database.username}`,
    {stdio: 'inherit'}
  )

  try {
    execSync(
      `PGPASSWORD=${Database.password} pg_restore -Fc ${sqlFile} -d ${Database.database} -h ${Database.host} -p ${Database.port} -U ${Database.username}`,
      {stdio: 'inherit'}
    )
  } catch (_err) {
    // Skip errors here because of the "must be owner of extension uuid-ossp" issue
  }

  console.log(new Date(), `${Database.database} database restore complete!`)
}

if (require.main === module) {
  main().catch((err) => {
    console.error(err)

    process.exit(1)
  })
}
