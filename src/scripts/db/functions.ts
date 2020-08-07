import path from 'path'
import readdir from 'recursive-readdir'
import {execSync} from 'child_process'

import Database from '../../config/Db'

export async function main(): Promise<void> {
  console.log(new Date(), `Refreshing DB functions...`)

  // Find all SQL files in the `src` folder
  const files = (
    await readdir(path.join(__dirname, '..', '..'))
  ).filter((filename) => filename.endsWith('.sql'))

  files.forEach((filename) => {
    console.log(new Date(), `Loading ${path.basename(filename)}...`)

    execSync(
      `PGPASSWORD=${Database.password} psql -h ${Database.host} -p ${Database.port} -U ${Database.username} -d ${Database.database} -f ${filename}`,
      {stdio: ['inherit', 'pipe', 'inherit']}
    )
  })
}

if (require.main === module) {
  main()
    .then(() => {
      process.exit(0)
    })
    .catch((err) => {
      console.error(err)

      process.exit(1)
    })
}
