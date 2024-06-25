using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;

namespace InformationSystem.BL.Facades;

public interface IStudentFacade : IFacade<StudentEntity, StudentListModel, StudentDetailModel>
{
    Task<StudentDetailModel?> GetNameAsync(string name);
    Task<StudentDetailModel?> GetSurnameAsync(string surname);
    Task<StudentDetailModel?> GetLoginAsync(string login);

    Task<List<StudentListModel>> GetStudentsSortedByNameAsync();
    Task<List<StudentListModel>> GetStudentsSortedBySurnameAsync();
    Task<List<StudentListModel>> GetStudentsSortedByLoginAsync();
}
