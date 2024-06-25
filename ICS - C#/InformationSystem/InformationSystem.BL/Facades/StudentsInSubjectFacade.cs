using InformationSystem.BL.Mappers;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Mappers;
using InformationSystem.DAL.Repositories;
using InformationSystem.DAL.UnitOfWork;

namespace InformationSystem.BL.Facades;

public class StudentsInSubjectFacade(
    IUnitOfWorkFactory unitOfWorkFactory,
    StudentsInSubjectModelMapper studentsInSubjectModelMapper)
    : FacadeBase<StudentsInSubjectEntity, StudentsInSubjectListModel, StudentsInSubjectDetailModel,
        StudentInSubjectEntityMapper>(unitOfWorkFactory, studentsInSubjectModelMapper), IStudentsInSubjectFacade
{
    public async Task SaveAsync(StudentsInSubjectListModel model, Guid studentId)
    {
        StudentsInSubjectEntity entity = studentsInSubjectModelMapper.MapToEntity(model, studentId);

        await using IUnitOfWork uow = UnitOfWorkFactory.Create();
        IRepository<StudentsInSubjectEntity> repository =
            uow.GetRepository<StudentsInSubjectEntity, StudentInSubjectEntityMapper>();

        if (await repository.ExistsAsync(entity))
        {
            await repository.UpdateAsync(entity);
            await uow.CommitAsync();
        }
    }

    public async Task SaveAsync(StudentsInSubjectDetailModel model, Guid studentId)
    {
        StudentsInSubjectEntity entity = studentsInSubjectModelMapper.MapToEntity(model, studentId);

        await using IUnitOfWork uow = UnitOfWorkFactory.Create();
        IRepository<StudentsInSubjectEntity> repository =
            uow.GetRepository<StudentsInSubjectEntity, StudentInSubjectEntityMapper>();

        repository.Insert(entity);
        await uow.CommitAsync();
    }
}