using InformationSystem.BL.Mappers;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Mappers;
using InformationSystem.DAL.Repositories;
using InformationSystem.DAL.UnitOfWork;

namespace InformationSystem.BL.Facades;

public class StudentEvaluationFacade(
    IUnitOfWorkFactory unitOfWorkFactory,
    StudentEvaluationModelMapper studentEvaluationModelMapper)
    :
        FacadeBase<StudentEvaluationEntity, StudentEvaluationListModel, StudentEvaluationDetailModel,
            StudentEvaluationEntityMapper>(unitOfWorkFactory, studentEvaluationModelMapper), IStudentEvaluationFacade
{
    public async Task SaveAsync(StudentEvaluationListModel model, Guid studentId)
    {
        StudentEvaluationEntity entity = studentEvaluationModelMapper.MapToEntity(model, studentId);

        await using IUnitOfWork uow = UnitOfWorkFactory.Create();
        IRepository<StudentEvaluationEntity> repository =
            uow.GetRepository<StudentEvaluationEntity, StudentEvaluationEntityMapper>();

        if (await repository.ExistsAsync(entity))
        {
            await repository.UpdateAsync(entity);
            await uow.CommitAsync();
        }
    }

    public async Task SaveAsync(StudentEvaluationDetailModel model, Guid studentId)
    {
        StudentEvaluationEntity entity = studentEvaluationModelMapper.MapToEntity(model, studentId);

        await using IUnitOfWork uow = UnitOfWorkFactory.Create();
        IRepository<StudentEvaluationEntity> repository =
            uow.GetRepository<StudentEvaluationEntity, StudentEvaluationEntityMapper>();

        repository.Insert(entity);
        await uow.CommitAsync();
    }
}