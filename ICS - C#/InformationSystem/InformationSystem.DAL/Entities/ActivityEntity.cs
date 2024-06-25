using InformationSystem.DAL.Enums;

namespace InformationSystem.DAL.Entities
{
    public record ActivityEntity : EntityBase
    {
        public required DateTime Start { get; set; }
        public required DateTime End { get; set; }
        public required Room Room { get; set; }
        public required ActivityType ActivityType { get; set; }
        public string? Description { get; set; }

        public required Guid SubjectId { get; set; }
        public required SubjectEntity Subject { get; init; }
    }
}