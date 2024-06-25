using InformationSystem.DAL.Enums;

namespace InformationSystem.BL.Models;

public record ActivityAndEvaluationListModel : ModelBase
{
    public required Guid SubjectId { get; set; }
    public required ActivityType ActivityType { get; set; }
    public required DateTime ActivityEnd { get; set; }
    public required uint ActivityPoints { get; set; }
    public string? ActivityNotes { get; set; }
    public string? ActivityEvaluator { get; set; }
    public SubjectDetailModel? Subject { get; set; }

    public static ActivityAndEvaluationListModel Empty => new()
    {
        Id = Guid.NewGuid(),
        SubjectId = Guid.Empty,
        ActivityType = ActivityType.None,
        ActivityEnd = DateTime.MinValue,
        ActivityPoints = 0,
        Subject = null
    };
}