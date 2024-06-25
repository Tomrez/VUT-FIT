using InformationSystem.DAL.Enums;

namespace InformationSystem.BL.Models;

public record StudentsInSubjectDetailModel : ModelBase
{
    public Guid SubjectId { get; set; }
    public required string SubjectName { get; set; }
    public required string SubjectAbbreviation { get; set; }

    public static StudentsInSubjectDetailModel Empty => new()
    {
        Id = Guid.NewGuid(),
        SubjectId = Guid.Empty,
        SubjectName = string.Empty,
        SubjectAbbreviation = string.Empty
    };
}