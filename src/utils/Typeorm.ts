import {
  Repository,
  DeepPartial,
  FindManyOptions,
  FindOneOptions,
  FindConditions,
  DeleteResult,
} from 'typeorm'
import {Source, pipe, map, fromValue, fromPromise} from 'wonka'

import {ManyResponse, paginateResponse} from './Pagination'

export interface QueryOptions<Entity> {
  where?: FindConditions<Entity>
  select?: (keyof Entity)[]
  order?: FindOneOptions<Entity>['order']
}

export interface ManyQueryOptions<Entity>
  extends Omit<QueryOptions<Entity>, 'where'> {
  where?: FindManyOptions<Entity>['where']
  pageSize?: number
  page?: number
}

export const find = <Entity>(repo: Repository<Entity>) => (
  options: ManyQueryOptions<Entity> = {}
): Source<ManyResponse<Entity>> => {
  const {where, order, pageSize, page} = options

  const skip =
    (pageSize && page && page > 1 && (page - 1) * pageSize) || undefined

  return pipe(
    fromPromise(
      repo.findAndCount({
        where,
        order,
        take: pageSize,
        skip,
      })
    ),
    map(([data, total]) =>
      paginateResponse(data, {
        total,
        pageSize,
        page,
      })
    )
  )
}

export const findOne = <Entity>(repo: Repository<Entity>) => ({
  where,
  order,
  select,
}: QueryOptions<Entity> = {}): Source<Entity | undefined> =>
  fromPromise(repo.findOne(where, {order, select}))

export const create = <Entity>(repo: Repository<Entity>) => (
  input: DeepPartial<Entity>
): Source<Entity | undefined> => {
  if (typeof input !== 'object' || !Object.keys(input).length) {
    return fromValue(undefined)
  }

  return fromPromise(repo.save(input))
}

export const update = <Entity>(repo: Repository<Entity>) => (
  id: string | number,
  input: DeepPartial<Entity>
): Source<Entity | undefined> => {
  if (typeof input !== 'object' || !Object.keys(input).length) {
    return fromValue(undefined)
  }

  return fromPromise(repo.save({...input, id}))
}

export const remove = <Entity>(repo: Repository<Entity>) => (
  id: string | number
): Source<DeleteResult> => fromPromise(repo.delete(id))

export const init = <Entity>(repo: Repository<Entity>) => ({
  find: find(repo),
  findOne: findOne(repo),
  create: create(repo),
  update: update(repo),
  delete: remove(repo),
})

export default {init}
