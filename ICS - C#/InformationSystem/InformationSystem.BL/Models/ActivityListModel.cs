using InformationSystem.DAL.Enums;

namespace InformationSystem.BL.Models;

public record ActivityListModel : ModelBase
{
    public required Guid SubjectId { get; set; }
    public required DateTime ActivityStart { get; set; }
    public required DateTime ActivityEnd { get; set; }
    public required Room ActivityRoom { get; set; }
    public required ActivityType ActivityType { get; set; }
    public SubjectDetailModel? Subject {  get; set; }
    public string? Description { get; set; }

    public static ActivityListModel Empty => new()
    {
        Id = Guid.NewGuid(),
        SubjectId = Guid.Empty,
        ActivityStart = DateTime.MinValue,
        ActivityEnd = DateTime.MinValue,
        ActivityRoom = Room.None,
        ActivityType = ActivityType.None,
        Subject = null
    };
}