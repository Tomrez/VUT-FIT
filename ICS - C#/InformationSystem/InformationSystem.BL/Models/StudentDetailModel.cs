using InformationSystem.DAL.Enums;
using System.Collections.ObjectModel;

namespace InformationSystem.BL.Models;

public record StudentDetailModel : ModelBase
{
    public required string Login { get; set; }
    public required string Name { get; set; }
    public required string Surname { get; set; }
    public Uri? PhotoUrl { get; set; }
    public ObservableCollection<StudentEvaluationListModel> StudentEvaluations { get; init; } = new();
    public ObservableCollection<StudentsInSubjectListModel> StudentsSubjects { get; init; } = new();

    public static StudentDetailModel Empty => new()
    {
        Id = Guid.NewGuid(), 
        Login = string.Empty, 
        Name = string.Empty, 
        Surname = string.Empty,
        PhotoUrl = null
    };
}