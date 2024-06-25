using InformationSystem.DAL.Enums;

namespace InformationSystem.BL.Models;

public record StudentsInSubjectListModel: ModelBase
{
    public Guid SubjectId { get; set; }
    public required string SubjectName { get; set; }
    public required string SubjectAbbreviation { get; set; }

    public static StudentsInSubjectListModel Empty => new()
    {
        Id = Guid.NewGuid(),
        SubjectId = Guid.Empty,
        SubjectName = string.Empty,
        SubjectAbbreviation = string.Empty
    };
}