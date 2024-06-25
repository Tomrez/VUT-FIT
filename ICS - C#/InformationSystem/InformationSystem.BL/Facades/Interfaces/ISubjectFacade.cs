using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;

namespace InformationSystem.BL.Facades;

public interface ISubjectFacade : IFacade<SubjectEntity, SubjectListModel, SubjectDetailModel>
{
    Task<SubjectDetailModel?> GetSubjectWithAbbreviationAsync(string abbreviation);
    Task<SubjectDetailModel?> GetSubjectWithNameAsync(string name);

    Task<List<SubjectListModel>> GetSubjectsSortedByAbbreviationAsync();
    Task<List<SubjectListModel>> GetSubjectsSortedByNameAsync();
}
