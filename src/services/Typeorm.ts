import camelcase from 'camelcase'
import {
  Repository,
  DeepPartial,
  FindManyOptions,
  FindOneOptions,
  FindConditions,
  DeleteResult,
} from 'typeorm'

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

export interface EntityService<Entity> {
  find: (options?: ManyQueryOptions<Entity>) => Promise<ManyResponse<Entity>>
  findOne: (options?: QueryOptions<Entity>) => Promise<Entity | undefined>
  create: (input: DeepPartial<Entity>) => Promise<Entity | undefined>
  update: (
    id: string | number,
    input: DeepPartial<Entity>
  ) => Promise<Entity | undefined>
  delete: (id: string | number) => Promise<DeleteResult>
}

export const fromOrderBy = <Entity, Order extends string>(
  orderBy?: Order[]
): FindOneOptions<Entity>['order'] =>
  orderBy?.reduce((memo, order) => {
    const index = order.lastIndexOf('_')
    const [field, direction] = [
      camelcase(order.substr(0, index)),
      order.substr(index + 1),
    ]

    return {...memo, [field]: direction}
  }, {})

export const find = <Entity>(repo: Repository<Entity>) => async (
  options: ManyQueryOptions<Entity> = {}
): Promise<ManyResponse<Entity>> => {
  const {where, order, pageSize, page} = options

  const skip =
    (pageSize && page && page > 1 && (page - 1) * pageSize) || undefined

  const [data, total] = await repo.findAndCount({
    where,
    order,
    take: pageSize,
    skip,
  })

  return paginateResponse(data, {
    total,
    pageSize,
    page,
  })
}

export const findOne = <Entity>(repo: Repository<Entity>) => async ({
  where,
  order,
  select,
}: QueryOptions<Entity> = {}): Promise<Entity | undefined> =>
  repo.findOne(where, {order, select})

export const create = <Entity>(repo: Repository<Entity>) => async (
  input: DeepPartial<Entity>
): Promise<Entity | undefined> => {
  if (typeof input !== 'object' || !Object.keys(input).length) {
    return undefined
  }

  return repo.save(input)
}

export const update = <Entity>(repo: Repository<Entity>) => async (
  id: string | number,
  input: DeepPartial<Entity>
): Promise<Entity | undefined> => {
  if (typeof input !== 'object' || !Object.keys(input).length) {
    return undefined
  }

  return repo.save({...input, id})
}

export const remove = <Entity>(repo: Repository<Entity>) => async (
  id: string | number
): Promise<DeleteResult> => repo.delete(id)

export const init = <Entity>(
  repo: Repository<Entity>
): EntityService<Entity> => ({
  find: find(repo),
  findOne: findOne(repo),
  create: create(repo),
  update: update(repo),
  delete: remove(repo),
})

export default {init}
