using InformationSystem.DAL.Enums;

namespace InformationSystem.BL.Models;

public record ActivityDetailModel : ModelBase
{
    public required Guid SubjectId { get; set; }
    public required DateTime ActivityStart { get; set; }
    public required DateTime ActivityEnd { get; set; }
    public required Room Room { get; set; }
    public required ActivityType ActivityType { get; set; }
    public string? Description { get; set; }

    public static ActivityDetailModel Empty => new()
    {
        Id = Guid.NewGuid(),
        SubjectId = Guid.Empty,
        ActivityStart = DateTime.MinValue,
        ActivityEnd = DateTime.MinValue,
        Room = Room.None,
        ActivityType = ActivityType.None
    };
}