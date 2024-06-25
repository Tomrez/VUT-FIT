using InformationSystem.BL.Mappers;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Mappers;
using InformationSystem.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.BL.Facades;

public class StudentFacade(
    IUnitOfWorkFactory unitOfWorkFactory,
    StudentModelMapper modelMapper)
    : FacadeBase<StudentEntity, StudentListModel, StudentDetailModel, StudentEntityMapper>(
        unitOfWorkFactory, modelMapper), IStudentFacade
{
    protected override ICollection<string> IncludesNavigationPathDetail =>
        new[] { $"{nameof(StudentEntity.Subjects)}.{nameof(StudentsInSubjectEntity.Subject)}",
                $"{nameof(StudentEntity.ActivityEvaluation)}.{nameof(StudentEvaluationEntity.Activity)}"};

    public virtual async Task<StudentDetailModel?> GetNameAsync(string name)
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<StudentEntity> query = uow.GetRepository<StudentEntity, StudentEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        StudentEntity? entity = await query.SingleOrDefaultAsync(e => e.Name == name).ConfigureAwait(false);

        return entity is null
            ? null
            : ModelMapper.MapToDetailModel(entity);
    }

    public virtual async Task<StudentDetailModel?> GetSurnameAsync(string surname)
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<StudentEntity> query = uow.GetRepository<StudentEntity, StudentEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        StudentEntity? entity = await query.SingleOrDefaultAsync(e => e.Surname == surname).ConfigureAwait(false);

        return entity is null
            ? null
            : ModelMapper.MapToDetailModel(entity);
    }

    public virtual async Task<StudentDetailModel?> GetLoginAsync(string login)
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<StudentEntity> query = uow.GetRepository<StudentEntity, StudentEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        StudentEntity? entity = await query.SingleOrDefaultAsync(e => e.Login == login).ConfigureAwait(false);

        return entity is null
            ? null
            : ModelMapper.MapToDetailModel(entity);
    }

    public virtual async Task<List<StudentListModel>> GetStudentsSortedByNameAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<StudentEntity> query = uow.GetRepository<StudentEntity, StudentEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<StudentEntity> sortedEntities = await query.OrderBy(e => e.Name).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }

    public virtual async Task<List<StudentListModel>> GetStudentsSortedBySurnameAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<StudentEntity> query = uow.GetRepository<StudentEntity, StudentEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<StudentEntity> sortedEntities = await query.OrderBy(e => e.Surname).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }

    public virtual async Task<List<StudentListModel>> GetStudentsSortedByLoginAsync()
    {
        await using IUnitOfWork uow = UnitOfWorkFactory.Create();

        IQueryable<StudentEntity> query = uow.GetRepository<StudentEntity, StudentEntityMapper>().Get();

        foreach (string includePath in IncludesNavigationPathDetail)
        {
            query = query.Include(includePath);
        }

        List<StudentEntity> sortedEntities = await query.OrderBy(e => e.Login).ToListAsync().ConfigureAwait(false);

        return sortedEntities.Select(ModelMapper.MapToListModel).ToList();
    }
}