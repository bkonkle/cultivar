import {Schema} from 'yup'
import {Source, pipe, mergeMap, fromValue, fromPromise} from 'wonka'

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
  errors: string[]
}

export type ValidationResult<Input> = ValidResult<Input> | InvalidResult<Input>

export const handleValidationResult = <Input, T = void>(handlers: {
  Valid: (result: ValidResult<Input>) => T
  Invalid: (result: InvalidResult<Input>) => T
}) => (result: ValidationResult<Input>): T => {
  switch (result.kind) {
    case ValidationResultKind.Valid:
      return handlers.Valid(result)
    case ValidationResultKind.Invalid:
      return handlers.Invalid(result)
  }
}

export const uuidRegex = /^[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i.compile()

export const valid = <Input>(input: Input): ValidResult<Input> => ({
  kind: ValidationResultKind.Valid,
  input,
})

export const invalid = <Input>(
  input: Input,
  errors: string[]
): InvalidResult<Input> => ({
  kind: ValidationResultKind.Invalid,
  input,
  errors,
})

export const validate = <Input>(schema: Schema<Input>) => async (
  input: Input
): Promise<ValidationResult<Input>> => {
  try {
    return valid(await schema.validate(input))
  } catch (err) {
    return invalid(input, err.errors)
  }
}

export const withValidation = <Input>(schema: Schema<Input>) =>
  mergeMap((input: Input) => fromPromise(validate(schema)(input)))

export const handleValidation = <Input, T = void>(
  input: Input,
  schema: Schema<Input>,
  handlers: {
    Valid: (result: ValidResult<Input>) => Source<T>
    Invalid: (result: InvalidResult<Input>) => Source<T>
  }
) =>
  pipe(
    fromValue(input),
    withValidation(schema),
    mergeMap(handleValidationResult(handlers))
  )

export const nothing = () => fromValue(undefined)
