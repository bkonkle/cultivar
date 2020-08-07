module.exports = {
  preset: 'ts-jest/presets/js-with-ts',
  collectCoverageFrom: ['src/**/*.{ts,gen.tsx}'],
  moduleDirectories: ['node_modules', 'src'],
  moduleFileExtensions: ['js', 'jsx', 'ts', 'tsx', 'json'],
  roots: ['<rootDir>/test'],
  testPathIgnorePatterns: ['/node_modules/', '/lib/'],
  testMatch: null,
  testRegex: '(/(test|__tests__)/.*(\\.|/|_)(test|spec))(\\.bs)?\\.[j|t]sx?$',
  testURL: 'http://localhost',
  transformIgnorePatterns: [
    // transform ES6 modules generated by BuckleScript
    // https://github.com/glennsl/bs-jest/issues/63
    '/node_modules/(?!(@.*/)?(bs-.*|reason-.*)/)(?!.+\\.bs\\.js).+\\.js$',
  ],
  globals: {
    'ts-jest': {
      diagnostics: false,
    },
  },
}
