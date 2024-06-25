using InformationSystem.DAL.Enums;

namespace InformationSystem.BL.Models;

public record StudentEvaluationDetailModel : ModelBase
{
    public required Guid ActivityId { get; set; }
    public required uint ActivityPoints { get; set; }
    public string? ActivityNotes { get; set; }
    public string? ActivityEvaluator { get; set; }

    public static StudentEvaluationDetailModel Empty => new()
    {
        Id = Guid.NewGuid(),
        ActivityId = Guid.Empty,
        ActivityPoints = 0
    };
}