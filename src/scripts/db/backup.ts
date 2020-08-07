import {execSync} from 'child_process'

import Database from '../../config/Db'

export const slugify = (text: string): string =>
  text
    .toString()
    .replace(/\s+/g, '-') // Replace spaces with -
    .replace(/[^\w\-]+/g, '-') // Replace all non-word chars with -
    .replace(/\-\-+/g, '-') // Replace multiple - with single -
    .replace(/^-+/, '') // Trim - from start of text
    .replace(/-+$/, '') // Trim - from end of text

async function main() {
  const timestamp = slugify(new Date().toISOString())

  console.log(new Date(), `Beginning ${Database.database} database backup...`)

  const sqlFile = process.argv[2] || `${Database.database}.${timestamp}.bak`

  execSync(
    `PGPASSWORD=${Database.password} pg_dump -Fc -f ${sqlFile} -d ${Database.database} -h ${Database.host} -p ${Database.port} -U ${Database.username}`,
    {stdio: 'inherit'}
  )

  console.log(new Date(), `${Database.database} database backup complete!`)
}

if (require.main === module) {
  main().catch((err) => {
    console.error(err)

    process.exit(1)
  })
}
