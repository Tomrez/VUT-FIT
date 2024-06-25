using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Enums;

namespace InformationSystem.BL.Mappers;

public class ActivityModelMapper()
    : ModelBaseMapper<ActivityEntity, ActivityListModel, ActivityDetailModel>
{
    public override ActivityListModel MapToListModel(ActivityEntity? entity)
        => entity is null
            ? ActivityListModel.Empty
            : new ActivityListModel
            {
                Id = entity.Id,
                SubjectId = entity.SubjectId,
                ActivityStart = entity.Start,
                ActivityEnd = entity.End,
                ActivityRoom = entity.Room,
                ActivityType = entity.ActivityType,
                Description = entity.Description
            };

    public override ActivityDetailModel MapToDetailModel(ActivityEntity? entity)
        => entity is null
            ? ActivityDetailModel.Empty
            : new ActivityDetailModel
            {
                Id = entity.Id,
                SubjectId = entity.SubjectId,
                ActivityStart = entity.Start,
                ActivityEnd = entity.End,
                Room = entity.Room,
                ActivityType = entity.ActivityType,
                Description = entity.Description
            };

    public override ActivityEntity MapToEntity(ActivityDetailModel model)
        => new()
        {
            Id = model.Id,
            SubjectId = model.SubjectId,
            Start = model.ActivityStart,
            End = model.ActivityEnd,
            Room = model.Room,
            ActivityType = model.ActivityType,
            Description = model.Description,
            Subject = null!
        };

    public ActivityEntity MapToEntity(ActivityListModel model, Room room)
        => new()
        {
            Id = model.Id,
            SubjectId = model.SubjectId,
            Start = model.ActivityStart,
            End = model.ActivityEnd,
            Room = room,
            ActivityType = model.ActivityType,
            Description = model.Description,
            Subject = null!
        };
}