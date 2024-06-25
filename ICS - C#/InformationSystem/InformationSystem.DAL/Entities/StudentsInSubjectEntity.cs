namespace InformationSystem.DAL.Entities
{
    public record StudentsInSubjectEntity : EntityBase
    {
        public Guid StudentId { get; set; }
        public Guid SubjectId { get; set; }

        public required StudentEntity Student { get; init; }
        public required SubjectEntity Subject { get; init; }
    }
}