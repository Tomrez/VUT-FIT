using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;

namespace InformationSystem.BL.Facades;

public interface IActivityFacade : IFacade<ActivityEntity, ActivityListModel, ActivityDetailModel>
{
    Task<ActivityDetailModel?> GetActivityStartAsync(Guid subjectId, DateTime activityStart);
    Task<ActivityDetailModel?> GetActivityEndAsync(Guid subjectId, DateTime activityEnd);

    Task<List<ActivityListModel>> GetActivitiesSortedByTypeAsync();
    Task<List<ActivityListModel>> GetActivitiesSortedByRoomAsync();
    Task<List<ActivityListModel>> GetActivitiesSortedByEndAsync();
    Task<List<ActivityListModel>> GetActivitiesSortedByStartAsync();
}
