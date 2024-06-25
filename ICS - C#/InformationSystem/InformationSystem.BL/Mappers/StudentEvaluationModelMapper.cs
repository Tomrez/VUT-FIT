using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;

namespace InformationSystem.BL.Mappers;

public class StudentEvaluationModelMapper :
    ModelBaseMapper<StudentEvaluationEntity, StudentEvaluationListModel, StudentEvaluationDetailModel>
{
    public override StudentEvaluationListModel MapToListModel(StudentEvaluationEntity? entity)
        => entity?.Student is null
            ? StudentEvaluationListModel.Empty
            : new StudentEvaluationListModel
            {
                Id = entity.Id,
                ActivityId = entity.Activity.Id,
                ActivityPoints = entity.Points,
                ActivityNotes = entity.Notes,
                ActivityEvaluator = entity.Evaluator
            };

    public override StudentEvaluationDetailModel MapToDetailModel(StudentEvaluationEntity? entity)
        => entity?.Student is null
            ? StudentEvaluationDetailModel.Empty
            : new StudentEvaluationDetailModel
            {
                Id = entity.Id,
                ActivityId = entity.Activity.Id,
                ActivityPoints = entity.Points,
                ActivityNotes = entity.Notes,
                ActivityEvaluator = entity.Evaluator
            };

    public StudentEvaluationListModel MapToListModel(StudentEvaluationDetailModel detailModel)
        => new()
        {
            Id = detailModel.Id,
            ActivityId = detailModel.ActivityId,
            ActivityPoints = detailModel.ActivityPoints,
            ActivityNotes = detailModel.ActivityNotes,
            ActivityEvaluator = detailModel.ActivityEvaluator
        };

    public void MapToExistingDetailModel(StudentEvaluationDetailModel existingDetailModel,
        StudentEvaluationListModel activity)
    {
        existingDetailModel.ActivityId = activity.Id;
        existingDetailModel.ActivityPoints = activity.ActivityPoints;
        existingDetailModel.ActivityNotes = activity.ActivityNotes;
        existingDetailModel.ActivityEvaluator = activity.ActivityEvaluator;
    }

    public override StudentEvaluationEntity MapToEntity(StudentEvaluationDetailModel model)
        => throw new NotImplementedException("This method is unsupported. Use the other overload.");

    
    public StudentEvaluationEntity MapToEntity(StudentEvaluationDetailModel model, Guid studentId)
        => new()
        {
            Id = model.Id,
            StudentId = studentId,
            ActivityId = model.ActivityId,
            Points = model.ActivityPoints,
            Notes = model.ActivityNotes,
            Evaluator = model.ActivityEvaluator,
            Activity = null!,
            Student = null!
        };

    public StudentEvaluationEntity MapToEntity(StudentEvaluationListModel model, Guid studentId)
        => new()
        {
            Id = model.Id,
            StudentId = studentId,
            ActivityId = model.ActivityId,
            Points = model.ActivityPoints,
            Notes = model.ActivityNotes,
            Evaluator = model.ActivityEvaluator,
            Activity = null!,
            Student = null!
        };
}