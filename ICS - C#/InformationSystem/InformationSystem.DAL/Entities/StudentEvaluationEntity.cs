namespace InformationSystem.DAL.Entities
{
    public record StudentEvaluationEntity : EntityBase
    {
        public required Guid StudentId { get; set; }
        public required Guid ActivityId { get; set; }
        public required uint Points { get; set; }
        public string? Notes { get; set; }
        public string? Evaluator { get; set;}

        public required ActivityEntity Activity { get; init; }
        public required StudentEntity Student { get; init; }
    }
}