using InformationSystem.BL.Mappers;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Mappers;
using InformationSystem.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.BL.Facades;

public class SubjectFacade(
    IUnitOfWorkFactory unitOfWorkFactory,
    SubjectModelMapper modelMapper)
    : FacadeBase<SubjectEntity, SubjectListModel, SubjectDetailModel, SubjectEntityMapper>(
        unitOfWorkFactory, modelMapper), ISubjectFacade
{
    protected override ICollection<string> IncludesNavigationPathDetail =>
        new[] { $"{nameof(SubjectEntity.Activities)}"};

    public virtual async Task<SubjectDetailModel?> GetSubjectWithAbbreviationAsync(string abbreviation)
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<SubjectEntity> query = uow.GetRepository<SubjectEntity, SubjectEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        SubjectEntity? entity = await query.SingleOrDefaultAsync(e => e.Abbreviation == abbreviation).ConfigureAwait(false);

        return entity is null
            ? null
            : ModelMapper.MapToDetailModel(entity);
    }

    public virtual async Task<SubjectDetailModel?> GetSubjectWithNameAsync(string name)
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<SubjectEntity> query = uow.GetRepository<SubjectEntity, SubjectEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        SubjectEntity? entity = await query.SingleOrDefaultAsync(e => e.Name == name).ConfigureAwait(false);

        return entity is null
            ? null
            : ModelMapper.MapToDetailModel(entity);
    }

    public virtual async Task<List<SubjectListModel>> GetSubjectsSortedByAbbreviationAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<SubjectEntity> query = uow.GetRepository<SubjectEntity, SubjectEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<SubjectEntity> sortedEntities = await query.OrderBy(e => e.Abbreviation).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }

    public virtual async Task<List<SubjectListModel>> GetSubjectsSortedByNameAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<SubjectEntity> query = uow.GetRepository<SubjectEntity, SubjectEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<SubjectEntity> sortedEntities = await query.OrderBy(e => e.Name).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }
}