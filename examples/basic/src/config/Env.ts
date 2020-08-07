import dotenv from 'dotenv'

dotenv.config()

export enum EnvKeys {
  NodeEnv = 'NODE_ENV',

  // Server
  Port = 'PORT',

  // Database
  DbHostname = 'DATABASE_HOSTNAME',
  DbUsername = 'DATABASE_USERNAME',
  DbPassword = 'DATABASE_PASSWORD',
  DbName = 'DATABASE_NAME',
  DbPort = 'DATABASE_PORT',
  DbDebugLogging = 'DATABASE_DEBUG_LOGGING',
  SshKeyPath = 'SSH_KEY_PATH',
  BastionHost = 'BASTION_HOST',
}

export const getEnv = (
  key: EnvKeys,
  defaultValue?: string,
  env: NodeJS.ProcessEnv = process.env
): string | undefined => env[key] || defaultValue
