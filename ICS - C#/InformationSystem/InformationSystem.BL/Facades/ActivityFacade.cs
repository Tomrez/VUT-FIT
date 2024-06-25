using InformationSystem.BL.Mappers;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Mappers;
using InformationSystem.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;
using System.Diagnostics;

namespace InformationSystem.BL.Facades;

public class ActivityFacade(
    IUnitOfWorkFactory unitOfWorkFactory,
    ActivityModelMapper modelMapper)
    : FacadeBase<ActivityEntity, ActivityListModel, ActivityDetailModel, ActivityEntityMapper>(unitOfWorkFactory, modelMapper),
        IActivityFacade
{
    public virtual async Task<ActivityDetailModel?> GetActivityStartAsync(Guid subjectId, DateTime activityStart)
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<ActivityEntity> query = uow.GetRepository<ActivityEntity, ActivityEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        ActivityEntity? entity = await query.SingleOrDefaultAsync(e => e.Start == activityStart && e.SubjectId == subjectId).ConfigureAwait(false);

        return entity is null
            ? null
            : ModelMapper.MapToDetailModel(entity);
    }

    public virtual async Task<ActivityDetailModel?> GetActivityEndAsync(Guid subjectId, DateTime activityEnd)
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<ActivityEntity> query = uow.GetRepository<ActivityEntity, ActivityEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        ActivityEntity? entity = await query.SingleOrDefaultAsync(e => e.End == activityEnd && e.SubjectId == subjectId).ConfigureAwait(false);

        return entity is null
            ? null
            : ModelMapper.MapToDetailModel(entity);
    }

    public virtual async Task<List<ActivityListModel>> GetActivitiesSortedByTypeAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<ActivityEntity> query = uow.GetRepository<ActivityEntity, ActivityEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<ActivityEntity> sortedEntities = await query.OrderBy(a => a.ActivityType).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }

    public virtual async Task<List<ActivityListModel>> GetActivitiesSortedByRoomAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<ActivityEntity> query = uow.GetRepository<ActivityEntity, ActivityEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<ActivityEntity> sortedEntities = await query.OrderBy(a => a.Room).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }

    public virtual async Task<List<ActivityListModel>> GetActivitiesSortedByEndAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<ActivityEntity> query = uow.GetRepository<ActivityEntity, ActivityEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<ActivityEntity> sortedEntities = await query.OrderBy(a => a.End).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }

    public virtual async Task<List<ActivityListModel>> GetActivitiesSortedByStartAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<ActivityEntity> query = uow.GetRepository<ActivityEntity, ActivityEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<ActivityEntity> sortedEntities = await query.OrderBy(a => a.Start).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }
}