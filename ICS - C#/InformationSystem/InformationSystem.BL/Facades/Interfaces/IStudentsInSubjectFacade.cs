using InformationSystem.BL.Models;

namespace InformationSystem.BL.Facades;

public interface IStudentsInSubjectFacade
{
    Task SaveAsync(StudentsInSubjectDetailModel model, Guid subjectId);
    Task SaveAsync(StudentsInSubjectListModel model, Guid subjectId);
    Task DeleteAsync(Guid id);
}