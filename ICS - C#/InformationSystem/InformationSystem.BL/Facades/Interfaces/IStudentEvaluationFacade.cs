using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;

namespace InformationSystem.BL.Facades;

public interface IStudentEvaluationFacade : IFacade<StudentEvaluationEntity, StudentEvaluationListModel, StudentEvaluationDetailModel>
{
    Task SaveAsync(StudentEvaluationDetailModel model, Guid studentId);
    Task SaveAsync(StudentEvaluationListModel model, Guid studentId);
}