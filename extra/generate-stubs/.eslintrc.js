module.exports = {
  root: true,
  env: {
    browser: true,
    es2021: true,
    node: true,
  },
  parser: '@typescript-eslint/parser',
  plugins: [
    '@typescript-eslint',
  ],
  extends: [
    'eslint:recommended',
    'plugin:@typescript-eslint/recommended',
  ],
  
  rules: {
    indent: ['error', 2, {
      'SwitchCase': 1,
      'flatTernaryExpressions': true,
      'ArrayExpression': 'first',
    }],
    semi: ['error', 'always'],
    curly: 'error',
    quotes: ['error', 'single', {'avoidEscape': true}],

    'brace-style': ['error', 'stroustrup', { allowSingleLine: true }],
    'block-spacing': ['warn', 'always'],
    'max-len': [
      'error',
      {
        code: 90,
        ignoreTemplateLiterals: true,
        ignoreComments: true,
        ignoreStrings: true
      }
    ],
    'eqeqeq': ['error', 'always'],
    'prefer-const': ['error', {
      'destructuring': 'any',
      'ignoreReadBeforeAssign': false
    }],  
    'operator-linebreak': ['error', 'before', { 'overrides': {
      '=': 'after',
      '+=': 'after',
      '-=': 'after',
      '||=': 'after',
      '&&=': 'after',
    } }],

    /* TypeScript Rules */
    '@typescript-eslint/no-inferrable-types': 1,
    '@typescript-eslint/no-explicit-any': 0,

    '@typescript-eslint/no-unused-vars': ['error', {
      'varsIgnorePattern': '^[iI]gnored?|^_',
      'argsIgnorePattern': '^[iI]gnored?|^_',
    }],
    '@typescript-eslint/no-empty-function': ['error', {
      'allow': ['private-constructors', 'protected-constructors'],
    }],
    '@typescript-eslint/no-this-alias': [
      'error',
      {
        'allowDestructuring': true,
      }
    ],

    /* Tell ESLint to stop complaining about "special" types that we know how
     * to use (and actually need) in type signatures.
     */
    '@typescript-eslint/ban-types': ['error', {
      'types': {
        '{}': false,
        'object': false,
      },
      'extendDefaults': true
    }],
  },
};
