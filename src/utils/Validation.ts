import Nope from 'nope-validator'
import {Source, fromValue, mergeMap} from 'wonka'

type NopeObject = ReturnType<typeof Nope['object']>
type NopeObjectErrors = ReturnType<NopeObject['validate']>

export enum ValidationResultKind {
  Valid = 'Valid',
  Invalid = 'Invalid',
}

export interface ValidResult<Input> {
  kind: ValidationResultKind.Valid
  input: Input
}

export interface InvalidResult<Input> {
  kind: ValidationResultKind.Invalid
  input: Input
  errors: NopeObjectErrors
}

export type ValidationResult<Input> = ValidResult<Input> | InvalidResult<Input>

export const uuidRegex = /^[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i.compile()

export const valid = <Input>(input: Input): ValidResult<Input> => ({
  kind: ValidationResultKind.Valid,
  input,
})

export const invalid = <Input>(
  input: Input,
  errors: NopeObjectErrors
): InvalidResult<Input> => ({
  kind: ValidationResultKind.Invalid,
  input,
  errors,
})

export const validate = (schema: NopeObject) => <Input>(
  input: Input
): ValidationResult<Input> => {
  const errors = schema.validate(input)

  if (errors) {
    return invalid(input, errors)
  }

  return valid(input)
}

export const fromValidation = (schema: NopeObject) => <Input>(
  input: Input
): Source<ValidationResult<Input>> => fromValue(validate(schema)(input))

export const handleValidationResult = <Input, T = void>(handlers: {
  Valid: (result: ValidResult<Input>) => Source<T>
  Invalid: (result: InvalidResult<Input>) => Source<T>
}) =>
  mergeMap(
    (result: ValidationResult<Input>): Source<T> => {
      switch (result.kind) {
        case ValidationResultKind.Valid:
          return handlers.Valid(result)
        case ValidationResultKind.Invalid:
          return handlers.Invalid(result)
      }
    }
  )
