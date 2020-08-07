import {EnvKeys, getEnv} from './Env'

const appName = 'storyverse'
const env = getEnv(EnvKeys.NodeEnv, 'production')
const dbHost = getEnv(EnvKeys.DbHostname, 'localhost')
const dbName = env === 'test' ? `${appName}_test` : appName

const config = {
  type: 'postgres',

  host: dbHost,
  port: Number(getEnv(EnvKeys.DbPort, '5432')),
  username: getEnv(EnvKeys.DbUsername, appName),
  password: getEnv(EnvKeys.DbPassword, appName),
  database: dbName,
}

export = config
